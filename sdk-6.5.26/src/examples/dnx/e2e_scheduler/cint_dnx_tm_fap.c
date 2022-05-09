/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_dnx_tm_fap.c
 * Purpose:     Example of simple E2E scheduler configuration where resource management is by SDK
 *
 * The example creates four HR schedulers and four voq connector groups for four ports.
 *
 * The settings include:
 *  o    Example delay tolerance configuration.
 *  o    Set module ID and enable traffic.
 *  o    Set up the association between system ports and gports.
 *  o    Create Scheduling hierarchy shown in Figure 4a. The setup is by specifying BCM_COSQ_GPORT_WITH_ID flag.
 *  o    Set bandwidth limitation of 500 Mbps on scheduler.
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   | creates 4 HR scheduler and 4 voq connector group for 4 ports  |
 *   |                                                               |
 *   |      E2E Hierarchy is as follows for sample port:             |
 *   |                                                               |
 *   |                  system port 10 maps to                       |
 *   |                             |                                 |
 *   |     EGRESS               port (1-4)                           |
 *   |                             |                                 |
 *   |        HR scheduler for port 1 created during soc init        |
 *   |                             |                                 |
 *   |                        HR scheduler                           |
 *   |                             |                                 |
 *   |                       SP0  SP1  SP2  SP4    no WFQ            |
 *   |     INGRESS                                                   |
 *   |                       connector group                         |
 *   |                      |    |    |    |                         |
 *   |                     cos0 cos1  cos2 cos3                      |
 *   |                 voq  8    9    10   11                        |
 *   |                                                               |
 *   |         +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+          |
 *   |         |   Figure 4a: Simple Scheduling Scheme    |          |
 *   |         |        Resource allocation by SDK        |          |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

int main(void)
{
    int unit = 0;
    int my_modid = 0;
    int idx, port, system_port;
    bcm_gport_t dest_gport[13];
    bcm_gport_t sys_gport[13];
    int num_cos = 8;
    int flags = 0;
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t gport_ucast_voq_connector_group[13];
    bcm_gport_t gport_ucast_queue_group[13];
    bcm_gport_t gport_ucast_scheduler[13];
    bcm_gport_t e2e_parent_gport[13];
    int ports[13];
    int voq;
    int NUM_PORTS = 6;
    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    bcm_gport_t e2e_gport;
    int max_burst, max_burst_factor, credit_worth;
    int cosq = 0;
    bcm_cosq_voq_connector_gport_t config;
    bcm_port_config_t port_config;
    bcm_pbmp_t pbmp;
    bcm_cosq_ingress_queue_bundle_gport_config_t conf;
    bcm_port_resource_t resource;
    int delay_tolerance_level, rate_class;
    bcm_cosq_scheduler_gport_params_t params;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
    max_burst_factor = credit_worth / 1024;
    /*Get all NIF ports*/
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get \n");
        return rv;
    }
    BCM_PBMP_ASSIGN(pbmp, port_config.nif);
    idx = 0;
    BCM_PBMP_ITER(pbmp, port) {
        ports[idx] = port;
        ++idx;
    }
    NUM_PORTS = idx;

    int modid_count, actual_modid_count;

    rv = bcm_stk_modid_count(unit, &modid_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stk_modid_count\n");
        return rv;
    }
    bcm_stk_modid_config_t modid_array[modid_count];

    rv = bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stk_modid_config_get_all\n");
        return rv;
    }
    my_modid = modid_array[0].modid;


    for (idx=0; idx<NUM_PORTS; idx++) {

        port = system_port = ports[idx];

        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
        BCM_GPORT_LOCAL_SET(dest_gport[idx], port);

        rv = bcm_stk_sysport_gport_set(unit, sys_gport[idx], dest_gport[idx]);

        if (rv != BCM_E_NONE) {
            printf("bcm_stk_sysport_gport_set failed $rv\n");
            return rv;
        }

        rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_get scheduler failed $rv\n");
            return rv;
        }

        /* create scheduler */

        params.type = bcmCosqSchedulerGportTypeHrSingleWfq;
        params.mode = bcmCosqSchedulerGportModeSimple;
        params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
        params.core = mapping_info.core;
        params.shared_order = bcmCosqSchedulerGportSharedOrderNone;

        rv = bcm_cosq_scheduler_gport_add(unit, 0, params, &gport_ucast_scheduler[idx]);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_scheduler_gport_add scheduler failed $rv\n");
            return rv;
        }

        /* COS 0 */
        rv = bcm_cosq_gport_sched_set(unit,
                gport_ucast_scheduler[idx],
                0,
                BCM_COSQ_SP1,
                0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }

        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);

        rv = bcm_cosq_gport_attach(unit, e2e_gport,
                gport_ucast_scheduler[idx], 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_attach failed $rv\n");
            return rv;
        }

        /* create voq connector */
        BCM_GPORT_LOCAL_SET(config.port, port);
        config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
        config.numq = num_cos;
        config.remote_modid = my_modid;
        config.nof_remote_cores = 2;
        rv = bcm_cosq_voq_connector_gport_add(unit, &config, &gport_ucast_voq_connector_group[idx]);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_voq_connector_gport_add connector failed $rv\n");
            return rv;
        }


        /* COS 0 */
        rv = bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                0,
                BCM_COSQ_SP0,
                0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }

        /* COS 1 */
        rv = bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                1,
                BCM_COSQ_SP1,
                0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }

        /* COS 2 */
        rv = bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                2,
                BCM_COSQ_SP2,
                0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }

        /* COS 3 */
        rv = bcm_cosq_gport_sched_set(unit,
                gport_ucast_voq_connector_group[idx],
                3,
                BCM_COSQ_SP4,
                0);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }

        for (cosq=0; cosq<4; cosq++) {
            rv = bcm_cosq_gport_attach(unit, gport_ucast_scheduler[idx],
                    gport_ucast_voq_connector_group[idx], cosq);
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_gport_attach failed $rv\n");
                return rv;
            }
        }

        flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;

        rv = bcm_port_resource_get(unit, port, &resource);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_resource_get failed $rv\n");
            return(rv);
        }
        rv = appl_dnx_e2e_voq_profiles_get(unit, resource.speed, &delay_tolerance_level, &rate_class);
        if (rv != BCM_E_NONE) {
            printf("appl_dnx_e2e_voq_profiles_get failed $rv\n");
            return(rv);
        }
        conf.flags = flags;
        conf.port = sys_gport[idx];
        conf.numq = num_cos;
        conf.local_core_id = BCM_CORE_ALL;
        for (cosq = 0; cosq < num_cos; cosq++) {
            conf.queue_atrributes[cosq].delay_tolerance_level = delay_tolerance_level;
            conf.queue_atrributes[cosq].rate_class = rate_class;
        }
        rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &conf, &gport_ucast_queue_group[idx]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_ingress_queue_bundle_gport_add failed\n");
            return rv;
        }
        printf("ucast gport(0x%08x)\n", gport_ucast_queue_group[idx]);


        /* connect voq to flow */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        connection.remote_modid = my_modid;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];

        rv = bcm_cosq_gport_connection_set(unit, &connection);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
            return rv;
        }

        /* connect voq to flow */
        int core;

        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.voq = gport_ucast_queue_group[idx];
        connection.voq_connector = gport_ucast_voq_connector_group[idx];

        connection.remote_modid = my_modid;

        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_set egress failed $rv\n");
            return rv;
        }

        /*
         * Set rate on the E2E Interface
         */
        kbits_sec_max = 1000000; /* 1Gbps */

        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);

        rv = bcm_fabric_port_get(unit,
                e2e_gport,
                0,
                &e2e_parent_gport[idx]);


        if (rv != BCM_E_NONE) {
            printf("bcm_fabric_port_get failed $rv\n");
            return rv;
        }

        rv = bcm_cosq_gport_bandwidth_set(unit,
                e2e_parent_gport[idx],
                0,
                0,
                kbits_sec_max,
                0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set if failed $rv\n");
            return rv;
        }


        /*
         * Set rate on the E2E port
         */
        kbits_sec_max = 500000; /* 500Mbps */

        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);

        rv = bcm_cosq_gport_bandwidth_set(unit,
                e2e_gport,
                0,
                0,
                kbits_sec_max,
                0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
            return rv;
        }


        /* Set rate on the Scheduler */
        kbits_sec_max = 500000; /* 500 Mbps */
        /* Scheduler */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                gport_ucast_scheduler[idx],
                0,
                0,
                kbits_sec_max,
                0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_set sched failed $rv\n");
            return rv;
        }

        max_burst = 3000 * max_burst_factor;

        /* Set max burst on the Scheduler */
        rv = bcm_cosq_control_set(unit,
                gport_ucast_scheduler[idx],
                0,
                bcmCosqControlBandwidthBurstMax,
                max_burst);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set max burst sched failed $rv\n");
            return rv;
        }

        max_burst = 3000 * max_burst_factor;

        for (cosq=0; cosq<4; cosq++) {
            /* Set rate on the Connector flow */
            rv = bcm_cosq_gport_bandwidth_set(unit,
                    gport_ucast_voq_connector_group[idx],
                    cosq,
                    0,
                    kbits_sec_max,
                    0);
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
                return rv;
            }

            /* Set maxburst on the Connector flow */
            rv = bcm_cosq_control_set(unit,
                    gport_ucast_voq_connector_group[idx],
                    cosq,
                    bcmCosqControlBandwidthBurstMax,
                    max_burst);

            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_control_set connector maxburst failed $rv\n");
                return rv;
            }
        }
        printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, gport_ucast_queue_group[idx]);
    }
    printf("cint_dnx_tm_fap.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

main();
