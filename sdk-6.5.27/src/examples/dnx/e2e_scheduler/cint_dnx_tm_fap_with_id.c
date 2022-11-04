/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_tm_fap_with_id.c
 * Purpose: Example script using WITH_ID flag in order
 *          to initial config for 4 ports.
 *
 * An example of setting up a specific hierarchy (BCM_COSQ_GPORT_WITH_ID usage)
 * The cint script overrides diag init cosq application (for those specific ports).
 * There is no need to skip the cosq application.
 *
 * The following configuration is being set:
 * 1. Create system port for local port 1-4.
 * 2. Setting up hierarchy for those ports.
 * 3. Set specific rates and max burst for the scheduler objects (0.5 Gbps).
 *
 * For detailed scheduled hierarchy see Negev UM document.
 * The cint script is very much similiar to cint_dnx_tm_fap_with_id.c where the main change
 * is by adding WITH_ID flag and giving a specific set of ids to the sceduling objects.
 *
 * Traffic example:
 * Run traffic to system port 10.
 * In order to set packet forwarding to system port 10, one needs to add suitable configuration.
 * Please see Negev UM for more details about that.
 * Expected results: For each port the maximum bandwidth is up to 0.5 GBps (as configured shaper).
 *
 * Enterance point:
 * Main function is example_setup.
 * Load the cint script, it calls automatically to example_setup();
 *
 * E2E Hierarchy being created,
 * as follows:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   | creates 4 HR scheduler and 4 voq connector group for 4 ports  |
 *   |                                                               |
 *   |      E2E Hierarchy is as follows for sample port:             |
 *   |                                                               |
 *   |                  system port 10 maps to                       |
 *   |                             |                                 |
 *   |     EGRESS               port (1-4)                           |
 *   |                             /\                                |
 *   |                             |                                 |
 *   |        HR scheduler for port 1 created during soc init        |
 *   |                             /\                                |
 *   |                             |                                 |
 *   |                        HR scheduler                           |
 *   |                             /\                                |
 *   |                             |                                 |
 *   |                       SP0  SP1  SP2  SP4    no WFQ            |
 *   |                       connector group                         |
 *   |                      /\   /\   /\   /\                        |
 *   |                   CID| CID| CID| CID|                         |
 *   |                   512| 513| 513| 513|                         |
 *   |     INGRESS                                                   |
 *   |                      |    |    |    |                         |
 *   |                     cos0 cos1  cos2 cos3                      |
 *   |                 voq 512  513   514  515                       |
 *   |                                                               |
 *   |         +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+          |
 *   |         |   Figure 4b: Simple Scheduling Scheme    |          |
 *   |         |    Resource allocation by Application    |          |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*
 * General Definitions
 */

int C1_NUM_PORTS = 4;
int C1_NUM_COS = 4;
int C1_QUEUE_FLAGS = (BCM_COSQ_GPORT_UCAST_QUEUE_GROUP);

int C1_E2E_INTERFACE_KBITS_SEC_MAX = 1000000; /* 1gbps */
int C1_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN = 500; /* 500 kbps */

int C1_E2E_PORT_KBITS_SEC_MAX = 500000; /* 500mbps */
int C1_E2E_PORT_KBITS_SEC_MAX_MARGIN = 10; /* 10 kbits */

int C1_SCHED_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C1_SCHED_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C1_SCHED_MAX_BURST = 3000;
int C1_SCHED_MAX_BURST_MARGIN = 75;
int C1_SCHED_MODE = BCM_COSQ_SP1;
int C1_SCHED_TYPE = bcmCosqSchedulerGportTypeHrSingleWfq;
int C1_CONN_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C1_CONN_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C1_CONN_MAX_BURST = 3000;
int C1_CONN_MAX_BURST_MARGIN = 75;
int C1_CONN_MODE[4] = { BCM_COSQ_SP0, BCM_COSQ_SP1, BCM_COSQ_SP2, BCM_COSQ_SP4 };
int C1_CONN_FLAGS = (BCM_COSQ_GPORT_VOQ_CONNECTOR);

/*
 * A specific set of IDS
 * Ingress/Egress are on the same device/unit
 */

/* Base VOQ Ids  on Ingress device for 4 ports/system ports. */
/* Each system port has a queue Group (4 Queues).            */
int c1_voq[4] = {512, 516, 520, 524};

/* VOQ connectors on egress device for 4 ports/system ports. */
/* Each sysetm port has a connector group (4 connectors).    */
/* This is corresponding to the queue group size (not a      */
/* composite configuration)                                  */
int c1_voq_connector[4] = {512, 516, 520, 524};

/* HR scheduler ID's on egress device, one for each port/system port. */

/**
 *
 * @param unit
 * @param scheds_array - pointer to an array with 4 elements for
 *                     the HRs to be allocated
 *
 * @return int
 */
int create_hrs_array(int unit, int *scheds_array)
{
    int hr_offset = 4, hr_region_start, first_hr_to_alloc, i;
    uint32 * first_hr_region;
    uint32 * region_size;

    first_hr_region = dnxc_data_get(unit, "sch", "flow", "first_hr_region", NULL);
    region_size = dnxc_data_get(unit, "sch", "flow", "region_size", NULL);

    /** Last two regions are reserved for HRs for Jr2 */
    /** Alocate from the last region to avoid possible conflicts */
    hr_region_start = (*first_hr_region) + 1;
    first_hr_to_alloc = hr_region_start * (*region_size) + 1;

    for (i = 0; i < 4; i++) {
        scheds_array[i] = (first_hr_to_alloc + (i * hr_offset ));
    }

    return BCM_E_NONE;
}
/**
 *
 * Expected configurations:
 *  Nof of  remote cores should be 1.
 *  dtm_flow_nof_remote_cores_region_X=1
 *
 * @return int
 */
int example_setup(void)
{
    int unit = 0;
    int my_modid = 0;
    int idx, port, system_port;
    bcm_gport_t dest_gport[4];
    bcm_gport_t sys_gport[4];
    int c1_sched[4];
    int num_cos = C1_NUM_COS;
    int flags = 0;
    bcm_gport_t gport_ucast_voq_connector_group[4];
    bcm_gport_t gport_ucast_queue_group[4];
    bcm_gport_t gport_ucast_scheduler[4];
    bcm_gport_t e2e_parent_gport[4];
    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    bcm_gport_t e2e_gport;
    int max_burst;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    /* A specific set of IDS */
    int cosq = 0, tm_port;
    uint32 port_flags;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;
    bcm_cosq_ingress_queue_bundle_gport_config_t conf;
    bcm_port_resource_t resource;
    int delay_tolerance_level, rate_class;
    bcm_cosq_scheduler_gport_params_t params;
    bcm_cosq_voq_connector_gport_t config;

    printf("\nSetup\n");

    BCM_IF_ERROR_RETURN_MSG(create_hrs_array(unit, c1_sched), "");

    /*
     * 1. Creation of system ports in the system
     * Iterate over ports 1-C1_NUM_PORTS.
     */
    for (idx=0; idx<C1_NUM_PORTS; idx++) {

        port = idx + 13;
        system_port = idx + 13;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &port_flags, &interface_info, &mapping_info), "");
        int core = mapping_info.core;
        tm_port = mapping_info.tm_port;
        BCM_GPORT_MODPORT_SET(dest_gport[idx], my_modid + core, tm_port);
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);

        printf("port(%d) core %d\n", port, core);

        BCM_IF_ERROR_RETURN_MSG(bcm_stk_sysport_gport_set(unit, sys_gport[idx], dest_gport[idx]), "");

        /*
         * 2. Creating Schedling Scheme
         * This is done by the following steps:
         *  1. Egress: Create for each local port: following HR, VOQ connector.
         *  2. Ingress: Create VOQ for each system port.
         *  3. Connect Ingress VOQ <=> Egress VOQ connectors.
         */

        /* Stage I: Egress Create HR scheduler, sched it and attach to suitable E2E port */

        BCM_GPORT_SCHEDULER_CORE_SET(gport_ucast_scheduler[idx], c1_sched[idx], core);;
        params.type = bcmCosqSchedulerGportTypeHrSingleWfq;
        params.mode = bcmCosqSchedulerGportModeSimple;
        params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
        params.core = core;
        params.shared_order = bcmCosqSchedulerGportSharedOrderNone;

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(unit, BCM_COSQ_SCHEDULER_GPORT_WITH_ID, params, &gport_ucast_scheduler[idx]), "");

        /* COS 0 */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit,
                gport_ucast_scheduler[idx],
                0,
                C1_SCHED_MODE,
                0), "");

        /* Attach HR as child to parent E2E port setup during soc init */
        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, e2e_gport,
                gport_ucast_scheduler[idx], 0), "");

        /* Stage I: Egress Create VOQ connector, sched it and attach to HR */

        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(gport_ucast_voq_connector_group[idx], c1_voq_connector[idx], core);
        config.port = port;
        config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_VOQ_CONNECTOR_GPORT_F_WITH_ID;
        config.numq = num_cos;
        config.remote_modid = my_modid;
        config.nof_remote_cores = 1;
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(unit, &config, &gport_ucast_voq_connector_group[idx]), "");
        /* Each VOQ connector attach suitable HR traffic class */
        /* COS 0 */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                0,
                C1_CONN_MODE[0],
                0), "");

        /* COS 1 */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                1,
                C1_CONN_MODE[1],
                0), "");

        /* COS 2 */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                2,
                C1_CONN_MODE[2],
                0), "");

        /* COS 3 */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                3,
                C1_CONN_MODE[3],
                0), "");

        /* Attach stage between VOQ connector and HR */
        for (cosq=0; cosq<num_cos; cosq++) {
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, gport_ucast_scheduler[idx],
                    gport_ucast_voq_connector_group[idx], cosq), "");
        }

        /* Stage 2: Ingress: Create VOQ for each system port. */
        flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
        /** Jr2 supports only Symetric Ingress mode so VOQ core
         *  should at all times be BCM_CORE_ALL */
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_ucast_queue_group[idx], BCM_CORE_ALL, c1_voq[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, port, &resource), "");
        BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_voq_profiles_get(unit, resource.speed, &delay_tolerance_level, &rate_class), "");
        conf.flags = flags;
        conf.port = sys_gport[idx];
        conf.numq = num_cos;
        conf.local_core_id = BCM_CORE_ALL;
        for (cosq = 0; cosq < num_cos; cosq++) {
            conf.queue_atrributes[cosq++].delay_tolerance_level = delay_tolerance_level;
            conf.queue_atrributes[cosq++].rate_class = rate_class;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_queue_bundle_gport_add(unit, &conf, &gport_ucast_queue_group[idx]), "");

        /**
         * Enable and configure per packet compensation
         */
        for (cosq=0; cosq<num_cos; cosq++) {
            /** Configure Compensation - constant value of 24 bytes (Preamable, IFG, CRC) assuming regular NIF port */
            adjust_info.gport = gport_ucast_queue_group[idx];
            adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;         /** compensation type*/
            adjust_info.flags = 0;
            adjust_info.cosq = cosq;
            adjust_info.source_info.source_id = 0;         /** irrelevant */

            /** Set per queue compensation */
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, 24), "");
        }


        /* bcm_cosq_ingress_queue_bundle_gport_add will return a VOQ gport with encode BCM_CORE_ALL.
           In order to be compliant with the asymmetric scheme created
           the VOQ gport is to be overwriten with the correct core */
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_ucast_queue_group[idx], core, c1_voq[idx]);
        printf("  ucast gport(0x%08x)\n", gport_ucast_queue_group[idx]);

        /* Stage 3: Connect Ingress VOQs <=> Egress VOQ connectors. */
        /* Ingress: connect voqs to voq connectors */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        connection.remote_modid = my_modid;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), "");

        /* Egress: connect voq connectors to voqs */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.remote_modid = my_modid + core;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), "");

        /*
         * 3. Set specific bandwidth and max burst for the scheduler objects.
         * Including: E2E interface, E2E port, VOQ connector.
         */


        /* Set rate on the E2E Interface */
        kbits_sec_max = C1_E2E_INTERFACE_KBITS_SEC_MAX; /* 1Gbps */

        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_port_get(unit,
                e2e_gport,
                0,
                &e2e_parent_gport[idx]), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit,
                e2e_parent_gport[idx],
                0,
                0,
                kbits_sec_max,
                0), "");


        /* Set rate on the E2E port */
        kbits_sec_max = C1_E2E_PORT_KBITS_SEC_MAX; /* 500Mbps */

        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit,
                e2e_gport,
                0,
                0,
                kbits_sec_max,
                0), "");


        /* Set rate on the Scheduler */
        kbits_sec_max = C1_SCHED_KBITS_SEC_MAX;
        /* Scheduler */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit,
                gport_ucast_scheduler[idx],
                0,
                0,
                kbits_sec_max,
                0), "");

        max_burst = 3000;

        /* Set max burst on the Scheduler */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit,
                gport_ucast_scheduler[idx],
                0,
                bcmCosqControlBandwidthBurstMax,
                max_burst), "");

        max_burst = 3000;

        for (cosq=0; cosq<4; cosq++) {
            /* Set rate on the VOQ Connector */
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit,
                    gport_ucast_voq_connector_group[idx],
                    cosq,
                    0,
                    kbits_sec_max,
                    0), "");

            /* Set maxburst on the VOQ Connector */
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit,
                    gport_ucast_voq_connector_group[idx],
                    cosq,
                    bcmCosqControlBandwidthBurstMax,
                    max_burst), "");

        }
        printf("  e2eGport(0x%x) schedGport(0x%x) connGport(0x%x) ucastGport(0x%x)\n",
                e2e_gport, gport_ucast_scheduler[idx],
                gport_ucast_voq_connector_group[idx], gport_ucast_queue_group[idx]);

        printf("  Setup Completed, port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, gport_ucast_queue_group[idx]);
    }

    printf("cint_dnx_tm_fap_with_id.c completed with status (%s)\n", bcm_errmsg(BCM_E_NONE));
    return BCM_E_NONE;
}

int example_teardown(int unit)
{
    int                           my_modid = 0;
    int                           idx, port, system_port;
    bcm_gport_t                   dest_gport[4];
    bcm_gport_t                   sys_gport[4];
    bcm_gport_t                   gport_ucast_voq_connector_group[4];
    bcm_gport_t                   gport_ucast_queue_group[4];
    bcm_gport_t                   gport_ucast_scheduler[4];
    bcm_gport_t                   e2e_gport[4];
    bcm_cosq_gport_connection_t   connection;
    int                           cosq, tm_port;
    uint32 port_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    char error_msg[200]={0,};

    printf("\nTeardown\n");

    for (idx = 0; idx < C1_NUM_PORTS; idx++) {

        /* form/retreive handles */
        port = idx + 13;
        system_port = idx + 13;
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &port_flags, &interface_info, &mapping_info), "");
        int core = mapping_info.core;
        tm_port = mapping_info.tm_port;
        BCM_GPORT_MODPORT_SET(dest_gport[idx], my_modid + core, tm_port);

        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport[idx], port);
        BCM_GPORT_SCHEDULER_CORE_SET(gport_ucast_scheduler[idx], c1_sched[idx], core);
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(gport_ucast_voq_connector_group[idx], c1_voq_connector[idx], core);
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_ucast_queue_group[idx], core, c1_voq[idx]);

        printf("port(%d)\n", port);

        printf("  e2eGport(0x%x) schedGport(0x%x) connGport(0x%x) ucastGport(0x%x)\n",
                e2e_gport[idx], gport_ucast_scheduler[idx],
                gport_ucast_voq_connector_group[idx], gport_ucast_queue_group[idx]);

        /* detach scheduler sub-tree from E2E hierarchy */
        snprintf(error_msg, sizeof(error_msg), "detach for SE(0x%x) failed", gport_ucast_scheduler[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_detach(unit, e2e_gport[idx], gport_ucast_scheduler[idx], 0), error_msg);
        printf("  detached schedGport(0x%x)\n", gport_ucast_scheduler[idx]);

        /* dis-associate queue from connectors */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = my_modid;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];

        snprintf(error_msg, sizeof(error_msg), "resetting queue2connector association failed, queue(0x%x)", gport_ucast_queue_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), error_msg);
        printf("  disconnected (Ingress) ucastGport(0x%x) from connGport(0x%x)\n", gport_ucast_queue_group[idx], gport_ucast_voq_connector_group[idx]);

        /* dis-associate connectors from queues */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = my_modid;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];

        snprintf(error_msg, sizeof(error_msg), "resetting connector2queue association failed, Connector(0x%x)", gport_ucast_voq_connector_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), error_msg);
        printf("  disconnected (Engress) connGport(0x%x) from ucastGport(0x%x)\n", gport_ucast_voq_connector_group[idx], gport_ucast_queue_group[idx]);

        /* delete queues */
        snprintf(error_msg, sizeof(error_msg), "delete for QueueGroup(0x%x) failed", gport_ucast_queue_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_delete(unit, gport_ucast_queue_group[idx]), error_msg);
        printf("  deleted ucastGport(0x%x)\n", gport_ucast_queue_group[idx]);

        /* detach connectors */
        for (cosq = 0; cosq < 4; cosq++) {
            snprintf(error_msg, sizeof(error_msg), "detach for Connector:cosq(0x%x:%d) failed", gport_ucast_voq_connector_group[idx]);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_detach(unit, gport_ucast_scheduler[idx],
                    gport_ucast_voq_connector_group[idx], cosq), error_msg);
        }
        printf("  detached connGport(0x%x) from schedGport(0x%x)\n", gport_ucast_voq_connector_group[idx], gport_ucast_scheduler[idx]);

        /* delete connectors */
        snprintf(error_msg, sizeof(error_msg), "delete for ConnectorGroup(0x%x) failed", gport_ucast_voq_connector_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_delete(unit, gport_ucast_voq_connector_group[idx]), error_msg);
        printf("  delete connGport(0x%x)\n", gport_ucast_voq_connector_group[idx]);

        /* detach/delete any Scheduler elements (SE) - none in this configuration */

        /* delete scheduler  element at top of sub-tree */
        snprintf(error_msg, sizeof(error_msg), "delete for SE(0x%x) failed", gport_ucast_scheduler[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_delete(unit, gport_ucast_scheduler[idx]), error_msg);
        printf("  delete schedGport(0x%x)\n", gport_ucast_scheduler[idx]);

        printf("  Teardown Completed, port(%d), ucastGport(0x%08x), connGport(0x%x), schedGport(0x%x)\n", port, gport_ucast_queue_group[idx], gport_ucast_voq_connector_group[idx], gport_ucast_scheduler[idx]);
    }

    printf("cint_dnx_tm_fap_with_id (teardown) completed with status (%s)\n", bcm_errmsg(BCM_E_NONE));

    return BCM_E_NONE;
}

int example_verify(int unit)
{
    int                           my_modid = 0;
    int                           idx, port, system_port;
    bcm_gport_t                   dest_gport[4];
    bcm_gport_t                   sys_gport[4];
    int                           c1_sched[4];
    bcm_gport_t                   gport_ucast_voq_connector_group[4];
    bcm_gport_t                   gport_ucast_queue_group[4];
    bcm_gport_t                   gport_ucast_scheduler[4];
    bcm_gport_t                   e2e_parent_gport[4];
    bcm_gport_t                   e2e_gport[4];
    int                           cosq, max_burst, slow_rate;
    int                           mode, weight;
    bcm_gport_t                   physical_port;
    int                           num_cos_levels, tm_port;
    uint32                        kbits_sec_min, kbits_sec_max, flags;
    bcm_cosq_gport_connection_t   connection;
    uint32 port_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_cosq_scheduler_gport_params_t params_get;
    bcm_cosq_voq_connector_gport_t config_get;
    char error_msg[200]={0,};

    printf("\nVerify Setup\n");

    BCM_IF_ERROR_RETURN_MSG(create_hrs_array(unit, c1_sched), "");

    for (idx = 0; idx < C1_NUM_PORTS; idx++) {

        /* form/retreive handles */
        port = idx + 13;
        system_port = idx + 13;
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &port_flags, &interface_info, &mapping_info), "");
        int core = mapping_info.core;
        tm_port = mapping_info.tm_port;
        BCM_GPORT_MODPORT_SET(dest_gport[idx], my_modid + core, tm_port);

        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport[idx], port);
        BCM_GPORT_SCHEDULER_CORE_SET(gport_ucast_scheduler[idx], c1_sched[idx], core);

        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(gport_ucast_voq_connector_group[idx], c1_voq_connector[idx], core);
        /** Jr2 supports only symetric Ingress mode so VOQ core
         *  should at all times be BCM_CORE_ALL */
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_ucast_queue_group[idx], BCM_CORE_ALL, c1_voq[idx]);

        printf("port(%d)\n", port);

        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_port_get(unit, e2e_gport[idx],0, &e2e_parent_gport[idx]), "");

        /* Get rate of the E2E Interface */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_get(unit,e2e_parent_gport[idx], 0, &kbits_sec_min, &kbits_sec_max, &flags), "");
        /* actual hardware value set returned */
        if ((kbits_sec_max > C1_E2E_INTERFACE_KBITS_SEC_MAX + C1_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN) ||
                (kbits_sec_max < C1_E2E_INTERFACE_KBITS_SEC_MAX - C1_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN)) {
            printf("E2E interface(0x%08x) bandwidth_get rate(%d) expected rate(1000000)\n", e2e_parent_gport[idx], kbits_sec_max);
        }

        /* Get rate on the E2E port */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_get(unit,e2e_gport[idx],0, &kbits_sec_min, &kbits_sec_max, &flags), "");
        if ((kbits_sec_max > C1_E2E_PORT_KBITS_SEC_MAX + C1_E2E_PORT_KBITS_SEC_MAX_MARGIN) ||
                (kbits_sec_max < C1_E2E_PORT_KBITS_SEC_MAX - C1_E2E_PORT_KBITS_SEC_MAX_MARGIN)) {
            printf("E2E interface(0x%08x) bandwidth_get rate(%d) expected rate(1000000)\n", e2e_parent_gport[idx], kbits_sec_max);
        }

        if (kbits_sec_max != C1_E2E_PORT_KBITS_SEC_MAX) {
            printf("E2E interface(0x%08x) bandwidth_get rate(%d) expected rate(500000)\n", e2e_gport[idx], kbits_sec_max);
        }

        /* retrive type of scheduling element */
        snprintf(error_msg, sizeof(error_msg), "failed on e2e Sched (0x%x)", e2e_gport[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_get(unit, 0, gport_ucast_scheduler[idx], &params_get), error_msg);
        if (params_get.type != C1_SCHED_TYPE) {
            printf("gport get mismatch on Sched (0x%x) expected type(%d) retreived type (%d)\n", gport_ucast_scheduler[idx], C1_SCHED_TYPE, params_get.type);
        }

        /* retreive SE scheduling discipline */
        snprintf(error_msg, sizeof(error_msg), "failed on Sched (0x%x)", gport_ucast_scheduler[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_get(unit, gport_ucast_scheduler[idx], 0, &mode, &weight), error_msg);
        if (mode != C1_SCHED_MODE) {
            printf("sched mode mismatch on Sched (0x%x) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", gport_ucast_scheduler[idx], (C1_SCHED_MODE - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
        }
        if (weight != 0) {
            printf("sched weight mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_ucast_scheduler[idx], 0, weight);
        }

        /* retreive rate of Scheduler */
        snprintf(error_msg, sizeof(error_msg), "failed on Sched (0x%x)", gport_ucast_scheduler[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_get(unit, gport_ucast_scheduler[idx], 0,
                &kbits_sec_min, &kbits_sec_max, &flags), error_msg);
        if ( (kbits_sec_max < (C1_SCHED_KBITS_SEC_MAX - C1_SCHED_KBITS_SEC_MAX_MARGIN)) ||
                (kbits_sec_max > (C1_SCHED_KBITS_SEC_MAX + C1_SCHED_KBITS_SEC_MAX_MARGIN)) ) {
            printf("bandwidth_get mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_ucast_scheduler[idx], C1_SCHED_KBITS_SEC_MAX, kbits_sec_max);
        }

        /* retreive scheduler burst size */
        snprintf(error_msg, sizeof(error_msg), "BandwidthBurstMax on Sched (0x%x) failed", gport_ucast_scheduler[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get(unit, gport_ucast_scheduler[idx], 0,
                bcmCosqControlBandwidthBurstMax, &max_burst), error_msg);
        if ( (max_burst < (C1_SCHED_MAX_BURST - C1_SCHED_MAX_BURST_MARGIN)) ||
                (max_burst > (C1_SCHED_MAX_BURST + C1_SCHED_MAX_BURST_MARGIN)) ) {
            printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_ucast_scheduler[idx], C1_SCHED_MAX_BURST, max_burst);
        }


        snprintf(error_msg, sizeof(error_msg), "failed on ucast queue group(0x%x)", gport_ucast_queue_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_get(unit, gport_ucast_queue_group[idx],
                &physical_port, &num_cos_levels, &flags), error_msg);

        if (flags != C1_QUEUE_FLAGS) {
            printf("gport get mismatch on ucast queue (0x%x) failed expected flags(0x%x) retrieved flags(0x%x)\n",
                    gport_ucast_queue_group[idx], C1_QUEUE_FLAGS, flags);
        }

        if (num_cos_levels != C1_NUM_COS) {
            printf("gport get mismatch on ucast queue (0x%x) expected numCos(%d) retreived numCos(%d)\n",
                    gport_ucast_queue_group[idx], C2_NUM_COS, num_cos_levels);
        }

        /* retreive type of connectors */
        snprintf(error_msg, sizeof(error_msg), "failed on Connector (0x%x)", gport_ucast_voq_connector_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_get(unit, gport_ucast_voq_connector_group[idx], &config_get), error_msg);
        if (config_get.flags != C1_CONN_FLAGS) {
            printf("gport get mismatch on Connector (0x%x) expected flags(0x%x) retreived flags (0x%x\n", gport_ucast_voq_connector_group[idx], C1_CONN_FLAGS, config_get.flags);
        }
        if (config_get.numq != C1_NUM_COS) {
            printf("gport get mismatch on Connector (0x%x) expected numCos(%d) retreived numCos(%d)\n", gport_ucast_voq_connector_group[idx], C1_NUM_COS, config_get.numq);
        }

        /* retreive rate of Connectors and corresponding burst size */
        for (cosq = 0; cosq < 4; cosq++) {
            /* retreive SE scheduling discipline */
            snprintf(error_msg, sizeof(error_msg), "failed on connector:cos (0x%x:%d)", gport_ucast_voq_connector_group[idx], cosq);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_get(unit, gport_ucast_voq_connector_group[idx], cosq, &mode, &weight), error_msg);
            if (mode != C1_CONN_MODE[cosq]) {
                printf("connector mode mismatch on connector:cos (0x%x:%d) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", gport_ucast_scheduler[idx], cosq, (C1_CONN_MODE[cosq] - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
            }
            if (weight != 0) {
                printf("connector weight mismatch on connector:cos (0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_scheduler[idx], cosq, 0, weight);
            }

            /* retreive rate on the VOQ Connector */
            snprintf(error_msg, sizeof(error_msg), "failed on connector:cos (0x%x:%d)", gport_ucast_voq_connector_group[idx], cosq);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_get(unit, gport_ucast_voq_connector_group[idx], cosq,
                    &kbits_sec_min, &kbits_sec_max, &flags), error_msg);

            if ( (kbits_sec_max < (C1_CONN_KBITS_SEC_MAX - C1_CONN_KBITS_SEC_MAX_MARGIN)) ||
                    (kbits_sec_max > (C1_CONN_KBITS_SEC_MAX + C1_CONN_KBITS_SEC_MAX_MARGIN)) ) {
                printf("bandwidth_get mismatch on connector:cosq (0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_voq_connector_group[idx], cosq, C1_CONN_KBITS_SEC_MAX, kbits_sec_max);
            }

            snprintf(error_msg, sizeof(error_msg), "BandwidthBurstMax on Connector:cosq (0x%x:%d) failed", gport_ucast_voq_connector_group[idx], cosq);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get(unit, gport_ucast_voq_connector_group[idx], cosq,
                    bcmCosqControlBandwidthBurstMax, &max_burst), error_msg);
            if ( (max_burst < (C1_CONN_MAX_BURST - C1_CONN_MAX_BURST_MARGIN)) ||
                    (max_burst > (C1_CONN_MAX_BURST + C1_CONN_MAX_BURST_MARGIN)) ) {
                printf("control_get-BandwidthBurstMax mismatch on Connector:cosq(0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_scheduler[idx], cosq, C1_SCHED_MAX_BURST, max_burst);
            }

            /* retreive VOQ Connector slow state */
            snprintf(error_msg, sizeof(error_msg), "FlowSlowRate on Connector:cosq (0x%x:%d) failed", gport_ucast_voq_connector_group[idx], cosq);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get(unit, gport_ucast_voq_connector_group[idx], cosq,
                    bcmCosqControlFlowSlowRate, &slow_rate), error_msg);
            if (slow_rate != 0) {
                printf("control_get-FlowSlowRate mismatch on Connector:cosq(0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_scheduler[idx], cosq, 0, slow_rate);
            }
        }

        /* retreive egress connection set information */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.voq_connector = gport_ucast_voq_connector_group[idx];
        snprintf(error_msg, sizeof(error_msg), "failed on Egress, Connector (0x%x)", gport_ucast_voq_connector_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_get(unit, &connection), error_msg);
        if (connection.voq != gport_ucast_queue_group[idx]) {
            printf("gport_connection_get on Egress, Connector (0x%x) VoQ association mismatch, Expected (0x%x), retreived (0x%x)\n", gport_ucast_voq_connector_group[idx], gport_ucast_queue_group[idx], connection.voq);
        }

        /* retreive ingress connection set information */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        connection.voq = gport_ucast_queue_group[idx];
        snprintf(error_msg, sizeof(error_msg), "failed on Ingress, VoQ (0x%x)", gport_ucast_queue_group[idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_get(unit, &connection), error_msg);
        if (connection.voq_connector != gport_ucast_voq_connector_group[idx]) {
            printf("gport_connection_get on Ingress, VoQ (0x%x) connector association mismatch, Expected (0x%x), retreived (0x%x)\n", gport_ucast_queue_group[idx], gport_ucast_voq_connector_group[idx], connection.voq_connector);
        }
    }

    printf("cint_dnx_tm_fap_with_id (verify) completed with status (%s)\n", bcm_errmsg(BCM_E_NONE));

    return BCM_E_NONE;
}

/* Traffic passes after this step */
example_setup();

/* Traffic stops after this step */
/* example_teardown(0); */

/* Traffic passes after this step */
/* example_setup(); */

/* verify setup */
example_verify(0);

