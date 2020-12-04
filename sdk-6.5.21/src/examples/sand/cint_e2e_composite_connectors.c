/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_e2e_composite_connectors.c
 * Purpose: Example of Composite Connector Validation
 *
 * Environment
 *    - Default Egress queue mapping in play.
 *    - Default Egress Port Scheduler hierarchy in play.
 *    - Default Ingress Scheduling Discipline (delay tolerance), queues sizes
 *    - Packet Size 1500 -  size should be aligned with credit source settings
 *
 * - Port Scheduler
 *   - CL Scheduler (SP mode)
 *     - 4 FQ Schedulers
 *       - 1 Queue Group / Connector Group (4 elements in bundle) - Composite
 *         1st Connector element (1st flow, CIR) connected to FQ-0
 *         1st Connector element (2nd flow, EIR) connected to FQ-2
 *         2nd Connector element (1st flow, CIR) connected to FQ-1
 *         2nd Connector element (2nd flow, EIR) connected to FQ-3
 *
 * - test run 1
 *   - Input
 *     - Queue - Cos 0
 *       Input 500 Mbps
 *     - Queue - Cos 1
 *       Input 500 Mbps
 *
 *   - Expected Output
 *     - Queue - Cos 0
 *       Expected Output 300 Mbps (CIR connector)
 *       Observed =>
 *     - Queue - Cos 1
 *       Expected Output 200 Mbps (CIR connector)
 *       Observed =>
 *
 * - test run 2
 *   - Input
 *     - Queue - Cos 0
 *       Input 500 Mbps
 *     - Queue - Cos 1
 *       Input 100 Mbps
 *
 *   - Expected Output
 *     - Queue - Cos 0
 *       Expected Output 300 Mbps (CIR connector)
 *       Observed =>
 *       Expected Output 100 Mbps (PIR connector)
 *       Observed =>
 *     - Queue - Cos 1
 *       Expected Output 100 Mbps (CIR connector)
 *       Observed =>
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                                |
 *  |                                      |                                         |
 *  |                                     SPR (500 Mbps)                             |
 *  |                                      |                                         |
 *  |                                     \/                                         |
 *  |                                    +----+                                      |
 *  |                                   /      \                                     |
 *  |                                  /   HR   \                                    |
 *  |                                 /          \                                   |
 *  |                                +-----------+                                   |
 *  |                                      |                                         |
 *  |                                     SPR (500 Mbps)                             |
 *  |                                      |                                         |
 *  |                                     \/                                         |
 *  |                              +----------------+                                |
 *  |                             / CL (4 SP levels) \                               |
 *  |                            /                    \                              |
 *  |                           / SP0   SP1  SP2   SP3 \                             |
 *  |                          +------------------------+                            |
 *  |                            /      \   /        \                               |
 *  |                           /        \ /          \                              |
 *  |                         SPR         /           SPR                            |
 *  |                    (300 Mbps)      / \        (200 Mbps)                       |
 *  |                       /          SPR SPR           \                           |
 *  |                      /     (200 Mbps) (300 Mbps)    \                          |
 *  |                    /            /       \            \                         |
 *  |                   \/          \/        \/           \/                        |
 *  |                 +------+   +------+   +------+   +------+                      |
 *  |                /   FQ   \ /   FQ   \ /   FQ   \ /   FQ   \                     |
 *  |               +----------+----------+----------+----------+                    |
 *  |                      \          /          \            /                      |
 *  |                       \        /            \          /                       |
 *  |                      SPR     SPR            SPR     SPR                        |
 *  |                 (300 Mbps)(200 Mbps)   (300 Mbps)(200 Mbps)                    |
 *  |                         \   /                  \   /                           |
 *  |                         \/ \/                  \/ \/                           |
 *  |                        |     |                |     |                          |
 *  |                        |     |                |     |                          |
 *  |                        |     |                |     |                          |
 *  |                        |     |                |     |                          |
 *  |                        |     |                |     |                          |
 *  |                        +-----+                +-----+      +----------------+  |
 *  |                     Input:500 Mbps        Input:100 Mbps   |      KEY       |  |
 *  |                                                            +----------------+  |
 *  |                                                            |SPR- Rate Shaper|  |
 *  |                                                            |                |  |
 *  |                                                            +----------------+  |
 *  |               +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                           |
 *  |               |   Figure 7: Composite Connectors   |                           |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note - JR2 required setting credit request profiles before creating VOQ.
 * So before running cint_e2e_composite_connectors.c cint, make sure to set the profiles:
 * cint src/examples/dnx/ingress_tm/cint_credit_request_profile.c
 * cint_credit_request_profile_backward_compatibilty_set(0, 4096);
 */

/*
 * General Definitions
 */
int C5_PETRA_CARD_NUM_PORTS = 1;
int C5_NUM_COS = 4;
int C5_DEFAULT_E2E_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED);

int C5_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP);
int C5_SCHEDULER_MODE = BCM_COSQ_SP1;
int C5_SCHED_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C5_SCHED_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C5_SCHED_MAX_BURST = 3000;
int C5_SCHED_MAX_BURST_MARGIN = 75;

int C5_E2E_INTERFACE_KBITS_SEC_MAX = 1000000; /* 1gbps */
int C5_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN = 500; /* 500 kbps */

int C5_NBR_FQ_SCHEDULERS = 4;
int C5_SCHED_FQ_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ);
int C5_SCHED_FQ0_MODE = BCM_COSQ_SP0;
int C5_SCHED_FQ1_MODE = BCM_COSQ_SP1;
int C5_SCHED_FQ2_MODE = BCM_COSQ_SP2;
int C5_SCHED_FQ3_MODE = BCM_COSQ_SP3;

int C5_CIR_KBITS_SEC_MAX = 300000; /* 300 Mbps */
int C5_CIR_KBITS_SEC_MAX_MARGIN = 10;
int C5_EIR_KBITS_SEC_MAX = 200000; /* 200 Mbps */
int C5_EIR_KBITS_SEC_MAX_MARGIN = 10;

int C5_CONN_FLAGS = (BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_COMPOSITE);
int C5_CONN_MODE = BCM_COSQ_SP0;

int C5_QUEUE_FLAGS = (BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_COMPOSITE);
/* handles */
int c5_my_modid = 0;
int credit_worth;
int is_dnx;

bcm_gport_t c5_gport_ucast_voq_connector_group0[32];
bcm_gport_t c5_gport_ucast_voq_connector_group0_sf2[32];
bcm_gport_t c5_gport_ucast_queue_group0[32];
bcm_gport_t c5_gport_ucast_scheduler[32];
bcm_gport_t c5_gport_ucast_scheduler_fq0[32];
bcm_gport_t c5_gport_ucast_scheduler_fq1[32];
bcm_gport_t c5_gport_ucast_scheduler_fq2[32];
bcm_gport_t c5_gport_ucast_scheduler_fq3[32];
bcm_gport_t c5_e2e_gport[32];
bcm_gport_t c5_e2e_parent_gport[32];
int c5_use_same_handles = 0;


void e2e_composite_connectors_display_handles(int idx)
{
    printf("  e2eGport(0x%x)\n", c5_e2e_gport[idx]);
    printf("    schedGport(0x%x)\n", c5_gport_ucast_scheduler[idx]);
    printf("    l3_fq0(0x%x) l3_fq1(0x%x) l3_fq2(0x%x) l3_fq3(0x%x)\n",
            c5_gport_ucast_scheduler_fq0[idx], c5_gport_ucast_scheduler_fq1[idx],
            c5_gport_ucast_scheduler_fq2[idx], c5_gport_ucast_scheduler_fq3[idx]);
    printf("    connGrp0Gport(0x%x) connGrp0Sf2Gport(0x%x)\n",
            c5_gport_ucast_voq_connector_group0[idx], c5_gport_ucast_voq_connector_group0_sf2[idx]);
    printf("    ucastGrp0Gport(0x%x)\n", c5_gport_ucast_queue_group0[idx]);
}


/* Added parameter: kbits_sec_max_port identify maximum bandwidth for the rate on the Scheduler and  E2E port. */
/* One example of running (see Negev UM for figure description is by running: main(500000) */
int e2e_composite_connectors_setup(int unit, int kbits_sec_max_port, int port_idx)
{
    int idx, port, system_port;
    bcm_gport_t dest_gport[32];
    bcm_gport_t sys_gport[32];
    int flags = 0, default_flags = 0;
    bcm_error_t rv = BCM_E_NONE;
    int voq;
    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    int max_burst;
    int cosq = 0, fq_sched = 0;
    bcm_cosq_delay_tolerance_t delay_tolerance;
    bcm_cosq_voq_connector_gport_t config;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info ;

/*
    rv = bcm_stk_my_modid_set(unit, c5_my_modid);
    if (rv != BCM_E_NONE) {
    printf("bcm_stk_my_modid_set failed $rv\n");
      return rv;
    }
*/

    printf("\nSetup\n");

    bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (is_dnx) {
        /* Default credit size for DPP was 1K.
         * In order to utilize the cint also for DNX device
         * where credit size by default is 4K
         * we multiply by 4 the MaxBurst
         */
        credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
        int multiplier = credit_worth / 1024;
        C5_SCHED_MAX_BURST *= multiplier;
        C5_SCHED_MAX_BURST_MARGIN *= multiplier;

        /* Some of the shapers of the DNX device's shapers are with bigger granularity
           because of their higher BW.
           That's why in order for the test to pass the rates are increased*/
        C5_E2E_INTERFACE_KBITS_SEC_MAX *= 10;
        C5_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN *= 10;
        C5_SCHED_KBITS_SEC_MAX *= 10;
        C5_SCHED_KBITS_SEC_MAX_MARGIN *= 10;
        C5_CIR_KBITS_SEC_MAX *= 10;
        C5_CIR_KBITS_SEC_MAX_MARGIN *= 10;
        C5_EIR_KBITS_SEC_MAX *= 10;
        C5_EIR_KBITS_SEC_MAX_MARGIN *= 10;
    }


    C5_SCHED_KBITS_SEC_MAX = kbits_sec_max_port;


    if (c5_use_same_handles == 1) {
        default_flags = BCM_COSQ_GPORT_WITH_ID;
        e2e_composite_connectors_display_handles(0);
    }


    /* for (idx=0; idx<C5_PETRA_CARD_NUM_PORTS; idx++) { */
    idx = port_idx;

    port = idx + 1;
    system_port = idx + 1;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
    BCM_GPORT_LOCAL_SET(dest_gport[idx], port);


    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rv\n");
        return rv;
    }

    rv = bcm_stk_sysport_gport_set(unit, sys_gport[idx], dest_gport[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return rv;
    }

    /* create CL scheduler */
    flags = C5_SCHED_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c5_gport_ucast_scheduler[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }


    /* COS 0, CL scheduler scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_scheduler[idx],
                      0,
                      C5_SCHEDULER_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    BCM_COSQ_GPORT_E2E_PORT_SET(c5_e2e_gport[idx], port);

        /* CL scheduler attach to E2E port scheduler */
    rv = bcm_cosq_gport_attach(unit, c5_e2e_gport[idx],
                   c5_gport_ucast_scheduler[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* create scheduler FQ scheduler (instance 0) - will aggregrate all cos 0 queues */
    flags = C5_SCHED_FQ_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c5_gport_ucast_scheduler_fq0[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 0) scheduling discipline configuration, weight of 2 */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_scheduler_fq0[idx],
                      0,
                      C5_SCHED_FQ0_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

        /* FQ scheduler (instance 0) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler[idx],
                   c5_gport_ucast_scheduler_fq0[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 1) - will aggregrate all cos 1 queues */
    flags = C5_SCHED_FQ_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c5_gport_ucast_scheduler_fq1[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 1) scheduling discipline configuration, weight of 4 */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_scheduler_fq1[idx],
                      0,
                      C5_SCHED_FQ1_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


        /* FQ scheduler (instance 1) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler[idx],
                   c5_gport_ucast_scheduler_fq1[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 2) - will aggregrate all cos 2 queues */
    flags = C5_SCHED_FQ_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c5_gport_ucast_scheduler_fq2[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 2) scheduling discipline configuration, weight of 8 */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_scheduler_fq2[idx],
                      0,
                      C5_SCHED_FQ2_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


        /* FQ scheduler (instance 2) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler[idx],
                   c5_gport_ucast_scheduler_fq2[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 3) - will aggregrate all cos 3 queues */
    flags = C5_SCHED_FQ_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c5_gport_ucast_scheduler_fq3[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 3) scheduling discipline configuration, weight of 16 */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_scheduler_fq3[idx],
                      0,
                      C5_SCHED_FQ3_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


        /* FQ scheduler (instance 3) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler[idx],
                   c5_gport_ucast_scheduler_fq3[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }




    /* create voq connector bundle, 4 elements in bundle */
    BCM_GPORT_LOCAL_SET(config.port, port);
    config.flags = C5_CONN_FLAGS | default_flags;
    config.numq = C5_NUM_COS;
    config.remote_modid = c5_my_modid + mapping_info.core;
    config.nof_remote_cores = 1;
    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &c5_gport_ucast_voq_connector_group0[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }

    /* handle for the 2nd flow (composite elements) */
    BCM_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(c5_gport_ucast_voq_connector_group0_sf2[idx], c5_gport_ucast_voq_connector_group0[idx], mapping_info.core);


    /* COS 0 (bundle 1st element, 1st Flow), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_voq_connector_group0[idx],
                      0,
                      C5_CONN_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element, 1st flow), attach to FQ scheduler (instance 0) */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler_fq0[idx],
               c5_gport_ucast_voq_connector_group0[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element, sub-Flow 2), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_voq_connector_group0_sf2[idx],
                      0,
                      C5_CONN_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element, sub-flow 2), attach to FQ scheduler (instance 2) */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler_fq2[idx],
               c5_gport_ucast_voq_connector_group0_sf2[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* COS 1, (bundle 2nd element, 1st flow), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_voq_connector_group0[idx],
                      1,
                      C5_CONN_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element, 1st flow), attach to FQ scheduler (instance 1) */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler_fq1[idx],
               c5_gport_ucast_voq_connector_group0[idx], 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element, sub-Flow 2), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c5_gport_ucast_voq_connector_group0_sf2[idx],
                      1,
                      C5_CONN_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element, sub-flow 2), attach to FQ scheduler (instance 3) */
    rv = bcm_cosq_gport_attach(unit, c5_gport_ucast_scheduler_fq3[idx],
               c5_gport_ucast_voq_connector_group0_sf2[idx], 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* create queue group bundle, 4 elements in bundle */
    flags = C5_QUEUE_FLAGS | default_flags;
    /** Creating VOQs only with system port gport is mandatory for JR2 (JR1 also supports it) */
    rv = bcm_cosq_gport_add(unit, sys_gport[idx], C5_NUM_COS,
                flags, &c5_gport_ucast_queue_group0[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue failed $rv\n");
        return rv;
    }
    printf("ucast gport(0x%08x)\n", c5_gport_ucast_queue_group0[idx]);

    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(c5_gport_ucast_queue_group0[idx], mapping_info.core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c5_gport_ucast_queue_group0[idx]));
    /* connect VoQ to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = c5_my_modid + mapping_info.core;
    connection.voq = c5_gport_ucast_queue_group0[idx];
    connection.voq_connector = c5_gport_ucast_voq_connector_group0[idx];

    rv = bcm_cosq_gport_connection_set(unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }

    /* connect flow to VoQ */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = c5_my_modid + mapping_info.core;
    connection.voq = c5_gport_ucast_queue_group0[idx];
    connection.voq_connector = c5_gport_ucast_voq_connector_group0[idx];

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }


    /*
     * Set rate on the E2E Interface
     */
    kbits_sec_max = C5_E2E_INTERFACE_KBITS_SEC_MAX; /* 1Gbps */

    BCM_COSQ_GPORT_E2E_PORT_SET(c5_e2e_gport[idx], port);

    rv = bcm_fabric_port_get(unit,
                      c5_e2e_gport[idx],
                      0,
                      &c5_e2e_parent_gport[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_fabric_port_get failed $rv\n");
        return rv;
    }

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_e2e_parent_gport[idx],
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
    BCM_COSQ_GPORT_E2E_PORT_SET(c5_e2e_gport[idx], port);

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_e2e_gport[idx],
                      0,
                      0,
                      kbits_sec_max_port,
                      0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return rv;
    }


    /* Set rate on the Scheduler */
    /* Scheduler */
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_gport_ucast_scheduler[idx],
                      0,
                      0,
                      kbits_sec_max_port,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return rv;
    }

    max_burst = C5_SCHED_MAX_BURST;

    /* Set max burst on the Scheduler */
    rv = bcm_cosq_control_set(unit,
                       c5_gport_ucast_scheduler[idx],
                       0,
                       bcmCosqControlBandwidthBurstMax,
                       max_burst);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return rv;
    }


    /*
     * FQ scheduler (instance 0)
     */
    kbits_sec_max = C5_CIR_KBITS_SEC_MAX;
    max_burst = C5_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_gport_ucast_scheduler_fq0[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                      c5_gport_ucast_scheduler_fq0[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * FQ scheduler (instance 1)
     */
    kbits_sec_max = C5_CIR_KBITS_SEC_MAX;
    max_burst = C5_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_gport_ucast_scheduler_fq1[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                      c5_gport_ucast_scheduler_fq1[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * FQ scheduler (instance 2)
     */
    kbits_sec_max = C5_EIR_KBITS_SEC_MAX;
    max_burst = C5_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_gport_ucast_scheduler_fq2[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                      c5_gport_ucast_scheduler_fq2[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * FQ scheduler (instance 3)
     */
    kbits_sec_max = C5_EIR_KBITS_SEC_MAX;
    max_burst = C5_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_gport_ucast_scheduler_fq3[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                      c5_gport_ucast_scheduler_fq3[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }


    /*
     * cos 0/1 = CIR
     */
    kbits_sec_max = C5_CIR_KBITS_SEC_MAX;
    max_burst = C5_SCHED_MAX_BURST;
    for (cosq = 0; cosq < 2; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_gport_ucast_voq_connector_group0[idx],
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
                      c5_gport_ucast_voq_connector_group0[idx],
                      cosq,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return rv;
        }
    }


    /*
     * cos 0/1 = EIR
     */
    kbits_sec_max = C5_EIR_KBITS_SEC_MAX;
    max_burst = C5_SCHED_MAX_BURST;
    for (cosq = 0; cosq < 2; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                      c5_gport_ucast_voq_connector_group0_sf2[idx],
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
                      c5_gport_ucast_voq_connector_group0_sf2[idx],
                      cosq,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return rv;
        }
    }

    if (c5_use_same_handles == 0) {
        e2e_composite_connectors_display_handles(idx);
    }

    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(c5_gport_ucast_queue_group0[idx], BCM_CORE_ALL, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c5_gport_ucast_queue_group0[idx]));

    printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, c5_gport_ucast_queue_group0[idx]);
    /* } */
    printf("cint_e2e_composite_connectors.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

int e2e_composite_connectors_teardown(int unit,int port_idx)
{
    int                           rc = BCM_E_NONE;
    int                           idx = port_idx;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_sched_fq_cir, gport_sched_fq_pir;
    bcm_gport_t                   gport_ucast_voq_conn_cir, gport_ucast_voq_conn_pir;
    int                           nbr_fq_sched;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;


    printf("\nTeardown\n");

    /*for (idx = 0; idx < C5_PETRA_CARD_NUM_PORTS; idx++) {*/

        e2e_composite_connectors_display_handles(idx);

        /* detach scheduler sub-tree from E2E hierarchy */
        rc = bcm_cosq_gport_detach(unit, c5_e2e_gport[idx], c5_gport_ucast_scheduler[idx], 0);
        if (BCM_FAILURE(rc)) {
            printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", c5_gport_ucast_scheduler[idx], rc, rc);
            return(rc);
        }
        printf("  detached schedGport(0x%x)\n", c5_gport_ucast_scheduler[idx]);


        /* dis-associate queue from connectors */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c5_my_modid + mapping_info.core;
        connection.voq = c5_gport_ucast_queue_group0[idx];
        connection.voq_connector = c5_gport_ucast_voq_connector_group0[idx];

        rc = bcm_cosq_gport_connection_set(unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting queue2connector association failed, queue(0x%x) Error (%d, 0x%x)\n", c5_gport_ucast_queue_group0[idx], rc, rc);
            return(rc);
        }
        printf("  disconnected (Ingress) ucastGport(0x%x) from connGport(0x%x)\n", c5_gport_ucast_queue_group0[idx], c5_gport_ucast_voq_connector_group0[idx]);


        /* dis-associate connectors from queues */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c5_my_modid + mapping_info.core;
        connection.voq = c5_gport_ucast_queue_group0[idx];
        connection.voq_connector = c5_gport_ucast_voq_connector_group0[idx];

        rc = bcm_cosq_gport_connection_set(unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting connector2queue association failed, Connector(0x%x) Error (%d, 0x%x)\n", c5_gport_ucast_voq_connector_group0[idx], rc, rc);
            return(rc);
        }
        printf("  disconnected (Engress) connGport(0x%x) from ucastGport(0x%x)\n", c5_gport_ucast_voq_connector_group0[idx], c5_gport_ucast_queue_group0[idx]);


        /* delete queues */
        rc = bcm_cosq_gport_delete(unit, c5_gport_ucast_queue_group0[idx]);
        if (BCM_FAILURE(rc)) {
            printf("delete for QueueGroup(0x%x) failed, Error (%d, 0x%x)\n", c5_gport_ucast_queue_group0[idx], rc, rc);
            return(rc);
        }
        printf("  deleted ucastGport(0x%x)\n", c5_gport_ucast_queue_group0[idx]);


        /* detach connectors */
        for (cosq = 0; cosq < 2; cosq++) {
            switch (cosq) {
                case 0:
                        gport_sched_fq_cir = c5_gport_ucast_scheduler_fq0[idx];
                        gport_sched_fq_pir = c5_gport_ucast_scheduler_fq2[idx];
                        gport_ucast_voq_conn_cir = c5_gport_ucast_voq_connector_group0[idx];
                        gport_ucast_voq_conn_pir = c5_gport_ucast_voq_connector_group0_sf2[idx];
                        break;
                default:
                        gport_sched_fq_cir = c5_gport_ucast_scheduler_fq1[idx];
                        gport_sched_fq_pir = c5_gport_ucast_scheduler_fq3[idx];
                        gport_ucast_voq_conn_cir = c5_gport_ucast_voq_connector_group0[idx];
                        gport_ucast_voq_conn_pir = c5_gport_ucast_voq_connector_group0_sf2[idx];
                        break;
            }

            rc = bcm_cosq_gport_detach(unit, gport_sched_fq_cir, gport_ucast_voq_conn_cir, cosq);
            if (BCM_FAILURE(rc)) {
                printf("detach for Connector:cosq(0x%x:%d) from Sched(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_voq_conn_cir, cosq, gport_sched_fq_cir, rc, rc);
                return(rc);
            }
            printf("  detached connGport:cosq(0x%x:%d) from schedGport(0x%x)\n", gport_ucast_voq_conn_cir, cosq, gport_sched_fq_cir);

            rc = bcm_cosq_gport_detach(unit, gport_sched_fq_pir, gport_ucast_voq_conn_pir, cosq);
            if (BCM_FAILURE(rc)) {
                printf("detach for Connector:cosq(0x%x:%d) from Sched(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_voq_conn_pir, cosq, gport_sched_fq_pir, rc, rc);
                return(rc);
            }
            printf("  detached connGport:cosq(0x%x:%d) from schedGport(0x%x)\n", gport_ucast_voq_conn_pir, cosq, gport_sched_fq_pir);
        }


        /* delete connectors */
        rc = bcm_cosq_gport_delete(unit, c5_gport_ucast_voq_connector_group0[idx]);
        if (BCM_FAILURE(rc)) {
            printf("delete for ConnectorGroup(0x%x) failed, Error (%d, 0x%x)\n", c5_gport_ucast_voq_connector_group0[idx], rc, rc);
            return(rc);
        }
        printf("  delete connGport(0x%x)\n", c5_gport_ucast_voq_connector_group0[idx]);


        /* detach intermediate Scheduler elements (SE) */
        for (nbr_fq_sched = 0; nbr_fq_sched < C5_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
            switch (nbr_fq_sched) {
                case 0: gport_scheduler_fq = c5_gport_ucast_scheduler_fq0[idx]; break;
                case 1: gport_scheduler_fq = c5_gport_ucast_scheduler_fq1[idx]; break;
                case 2: gport_scheduler_fq = c5_gport_ucast_scheduler_fq2[idx]; break;
                default: gport_scheduler_fq = c5_gport_ucast_scheduler_fq3[idx]; break;
            }

            rc = bcm_cosq_gport_detach(unit, c5_gport_ucast_scheduler[idx], gport_scheduler_fq, 0);
            if (BCM_FAILURE(rc)) {
                printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
                return(rc);
            }
            printf("  detached schedGport(0x%x)\n", gport_scheduler_fq);
        }


        /* delete intermediate Scheduler elements (SE) */
        for (nbr_fq_sched = 0; nbr_fq_sched < C5_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
            switch (nbr_fq_sched) {
                case 0: gport_scheduler_fq = c5_gport_ucast_scheduler_fq0[idx]; break;
                case 1: gport_scheduler_fq = c5_gport_ucast_scheduler_fq1[idx]; break;
                case 2: gport_scheduler_fq = c5_gport_ucast_scheduler_fq2[idx]; break;
                default: gport_scheduler_fq = c5_gport_ucast_scheduler_fq3[idx]; break;
            }

            rc = bcm_cosq_gport_delete(unit, gport_scheduler_fq);
            if (BCM_FAILURE(rc)) {
                printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
                return(rc);
            }
            printf("  delete schedGport(0x%x)\n", gport_scheduler_fq);
        }


        /* delete scheduler  element at top of sub-tree */
        rc = bcm_cosq_gport_delete(unit, c5_gport_ucast_scheduler[idx]);
        if (BCM_FAILURE(rc)) {
            printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", c5_gport_ucast_scheduler[idx], rc, rc);
            return(rc);
        }
        printf("  delete schedGport(0x%x)\n", c5_gport_ucast_scheduler[idx]);

    /*} end of for loop */

    printf("cint_e2e_composite_connectors.c (teardown) completed with status (%s)\n", bcm_errmsg(rc));

    c5_use_same_handles = 1;

    return(rc);
}

int
e2e_composite_connectors_verify_flags(int unit, bcm_gport_t gport, int exp_flags)
{
    int                           rc = BCM_E_NONE;
    int                           mode, weight;
    bcm_gport_t                   physical_port;
    int                           num_cos_levels;
    uint32                        flags;

    /* retreive type of scheduling element */
    rc = bcm_cosq_gport_get(unit, gport, &physical_port, &num_cos_levels, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", gport, rc);
        return(rc);
    }
    if (flags != exp_flags) {
        printf("gport get mismatch on gport (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", gport, exp_flags, flags);
    }

    return(rc);
}

int
e2e_composite_connectors_verify_mode(int unit, bcm_gport_t gport, int cosq, int exp_mode, int exp_weight)
{
    int                           rc = BCM_E_NONE;
    int                           mode, weight;


    rc = bcm_cosq_gport_sched_get(unit, gport, cosq, &mode, &weight);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_get on gport (0x%x) failed, error (%d)\n", gport_ucast_scheduler, rc);
        return(rc);
    }
    if (mode != exp_mode) {
        printf("sched mode mismatch on gport (0x%x) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", gport, (exp_mode - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
    }
    if (weight != exp_weight) {
        printf("sched weight mismatch on gport (0x%x) expected(%d) retreived (%d)\n", gport, 0, weight);
    }

    return(rc);
}

int
e2e_composite_connectors_verify_bandwidth(int unit, bcm_gport_t gport, int cosq, int exp_rate, int margin)
{
    int                             rc = BCM_E_NONE;
    uint32                          kbits_sec_max, kbits_sec_min, flags;

    rc = bcm_cosq_gport_bandwidth_get(unit, gport, cosq, &kbits_sec_min, &kbits_sec_max, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_get on gport (0x%x) failed, error (%d)\n", gport, rc);
        return(rc);
    }
    if ( (kbits_sec_max < (exp_rate - margin)) ||
         (kbits_sec_max > (exp_rate + margin)) ) {
        printf("bandwidth_get mismatch on gport (0x%x) expected(%d) retreived (%d)\n", gport, exp_rate, kbits_sec_max);
    }

    return(rc);
}

int
e2e_composite_connectors_verify_burst_size(int unit, bcm_gport_t gport, int cosq, int exp_burst_size, int margin)
{
    int                           rc = BCM_E_NONE;
    int                           max_burst;


    rc = bcm_cosq_control_get(unit, gport, cosq, bcmCosqControlBandwidthBurstMax, &max_burst);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_control_get-BandwidthBurstMax on gport (0x%x) failed, error (%d)\n", gport, rc);
        return(rc);
    }
    if ( (max_burst < (exp_burst_size - margin)) || (max_burst > (exp_burst_size + margin)) ) {
        printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport, exp_burst_size, max_burst);
    }

    return(rc);
}

/* verify setup */
int e2e_composite_connectors_verify(int unit, int kbits_sec_max_port, int port_idx)
{
    int                           rc = BCM_E_NONE;
    int                           idx = port_idx;
    bcm_port_t                    port = port_idx + 1;
    bcm_gport_t                   physical_port;
    int                           num_cos_levels;
    uint32                        flags;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_ucast_voq_conn;
    int                           nbr_fq_sched, nbr_flows;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;
    int                           exp_flags, exp_mode, exp_margin, exp_kbits_sec_max;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info ;

    printf("\nVerify Setup\n");

    rc = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rc != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rc\n");
        return rc;
    }

    /*for (idx = 0; idx < C5_PETRA_CARD_NUM_PORTS; idx++) {*/

        exp_margin = C5_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN;
        rc = e2e_composite_connectors_verify_bandwidth(unit, c5_e2e_parent_gport[idx], 0, C5_E2E_INTERFACE_KBITS_SEC_MAX, exp_margin);
        if (rc != BCM_E_NONE) {
            return(rc);
        }
        exp_margin = kbits_sec_max_port /100; /* 1% error */
        rc = e2e_composite_connectors_verify_bandwidth(unit, c5_e2e_gport[idx], 0, kbits_sec_max_port, exp_margin);
        if (rc != BCM_E_NONE) {
            return(rc);
        }
        /* retrive e2e port scheduling element */
        rc = bcm_cosq_gport_get(unit, c5_e2e_gport[idx], &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on e2e Sched (0x%x) failed, error (%d)\n", c5_e2e_gport[idx], rc);
            return(rc);
        }
        if (flags != C5_DEFAULT_E2E_SCHED_FLAGS) {
            printf("gport get mismatch on E2E Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", c5_e2e_gport[idx], C5_DEFAULT_E2E_SCHED_FLAGS, flags);
        }

        /* retreive type of scheduling element */
        rc = e2e_composite_connectors_verify_flags(unit, c5_gport_ucast_scheduler[idx], C5_SCHED_FLAGS);
        if (rc != BCM_E_NONE) {
            return(rc);
        }

        /* retreive SE scheduling discipline */
        rc = e2e_composite_connectors_verify_mode(unit, c5_gport_ucast_scheduler[idx], 0, C5_SCHEDULER_MODE, 0);
        if (rc != BCM_E_NONE) {
            return(rc);
        }

        /* retreive rate of Scheduler */
        rc = e2e_composite_connectors_verify_bandwidth(unit, c5_gport_ucast_scheduler[idx], 0, C5_SCHED_KBITS_SEC_MAX, C5_SCHED_KBITS_SEC_MAX_MARGIN);
        if (rc != BCM_E_NONE) {
            return(rc);
        }

        /* retreive scheduler burst size */
        rc = e2e_composite_connectors_verify_burst_size(unit, c5_gport_ucast_scheduler[idx], 0, C5_SCHED_MAX_BURST, C5_SCHED_MAX_BURST_MARGIN);
        if (rc != BCM_E_NONE) {
            return(rc);
        }

        /* retreive intermediate Scheduler elements (SE) */
        for (nbr_fq_sched = 0; nbr_fq_sched < C5_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
            switch (nbr_fq_sched) {
                case 0: gport_scheduler_fq = c5_gport_ucast_scheduler_fq0[idx];
                        exp_mode = C5_SCHED_FQ0_MODE;
                        exp_kbits_sec_max = C5_CIR_KBITS_SEC_MAX;
                        exp_margin = C5_CIR_KBITS_SEC_MAX_MARGIN; break;
                case 1: gport_scheduler_fq = c5_gport_ucast_scheduler_fq1[idx];
                        exp_mode = C5_SCHED_FQ1_MODE;
                        exp_kbits_sec_max = C5_CIR_KBITS_SEC_MAX;
                        exp_margin = C5_CIR_KBITS_SEC_MAX_MARGIN; break;
                case 2: gport_scheduler_fq = c5_gport_ucast_scheduler_fq2[idx];
                        exp_mode = C5_SCHED_FQ2_MODE;
                        exp_kbits_sec_max = C5_EIR_KBITS_SEC_MAX;
                        exp_margin = C5_EIR_KBITS_SEC_MAX_MARGIN; break;
                default: gport_scheduler_fq = c5_gport_ucast_scheduler_fq3[idx];
                        exp_mode = C5_SCHED_FQ3_MODE;
                        exp_kbits_sec_max = C5_EIR_KBITS_SEC_MAX;
                        exp_margin = C5_EIR_KBITS_SEC_MAX_MARGIN; break;
            }

            /* retreive type of scheduling element */
            rc = e2e_composite_connectors_verify_flags(unit, gport_scheduler_fq, C5_SCHED_FQ_FLAGS);
            if (rc != BCM_E_NONE) {
                return(rc);
            }

            /* retreive SE scheduling discipline */
            rc = e2e_composite_connectors_verify_mode(unit, gport_scheduler_fq, 0, exp_mode, 0);
            if (rc != BCM_E_NONE) {
                return(rc);
            }

            /* retreive rate of Scheduler */
            rc = e2e_composite_connectors_verify_bandwidth(unit, gport_scheduler_fq, 0, exp_kbits_sec_max, exp_margin);
            if (rc != BCM_E_NONE) {
                return(rc);
            }

            /* retreive scheduler burst size */
            rc = e2e_composite_connectors_verify_burst_size(unit, gport_scheduler_fq, 0, C5_SCHED_MAX_BURST, C5_SCHED_MAX_BURST_MARGIN);
            if (rc != BCM_E_NONE) {
                return(rc);
            }
        }

        /* retreive type of connector element */
        rc = e2e_composite_connectors_verify_flags(unit, c5_gport_ucast_voq_connector_group0[idx], C5_CONN_FLAGS);
        if (rc != BCM_E_NONE) {
            return(rc);
        }

        /* retreive connector attributes*/
        for (cosq = 0; cosq < 2; cosq++) {
            for (nbr_flows = 0; nbr_flows < 2; nbr_flows++) {
                switch (nbr_flows) {
                    case 0:
                        gport_ucast_voq_conn = c5_gport_ucast_voq_connector_group0[idx];
                        exp_kbits_sec_max = C5_CIR_KBITS_SEC_MAX;
                        exp_margin = C5_CIR_KBITS_SEC_MAX_MARGIN;
                        break;
                    default:
                        gport_ucast_voq_conn = c5_gport_ucast_voq_connector_group0_sf2[idx];
                        exp_kbits_sec_max = C5_EIR_KBITS_SEC_MAX;
                        exp_margin = C5_EIR_KBITS_SEC_MAX_MARGIN;
                        break;
                }

                /* retreive SE scheduling discipline */
                rc = e2e_composite_connectors_verify_mode(unit, gport_ucast_voq_conn, cosq, C5_CONN_MODE, 0);
                if (rc != BCM_E_NONE) {
                    return(rc);
                }

                /* retreive rate of Scheduler */
                rc = e2e_composite_connectors_verify_bandwidth(unit, gport_ucast_voq_conn, cosq, exp_kbits_sec_max, exp_margin);
                if (rc != BCM_E_NONE) {
                    return(rc);
                }

                /* retreive scheduler burst size */
                rc = e2e_composite_connectors_verify_burst_size(unit, gport_ucast_voq_conn, cosq, C5_SCHED_MAX_BURST, C5_SCHED_MAX_BURST_MARGIN);
                if (rc != BCM_E_NONE) {
                    return(rc);
                }
            }
        }

        rc = bcm_cosq_gport_get(unit, c5_gport_ucast_queue_group0[idx],
                    &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on ucast queue group(0x%x) failed, error (%d)\n", gport_ucast_queue_group, rc);
            return(rc);
        }

        if (flags != C5_QUEUE_FLAGS) {
            printf("gport get mismatch on ucast queue (0x%x) failed expected flags(0x%x) retrieved flags(0x%x)\n",
                   gport_ucast_queue_group, C5_QUEUE_FLAGS, flags);
        }

        if (num_cos_levels != C5_NUM_COS) {
            printf("gport get mismatch on ucast queue (0x%x) expected numCos(%d) retreived numCos(%d)\n",
                   gport_ucast_queue_group, C5_NUM_COS, num_cos_levels);
        }

        /* retreive egress connection set information */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.voq_connector = c5_gport_ucast_voq_connector_group0[idx];
        rc = bcm_cosq_gport_connection_get(unit, &connection);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_get on Egress, Connector (0x%x) failed, error (%d)\n", c5_gport_ucast_voq_connector_group0[idx], rc);
            return(rc);
        }
        if (connection.voq != c5_gport_ucast_queue_group0[idx]) {
            printf("gport_connection_get on Egress, Connector (0x%x) VoQ association mismatch, Expected (0x%x), retreived (0x%x)\n", c5_gport_ucast_voq_connector_group0[idx], c5_gport_ucast_queue_group0[idx], connection.voq);
        }

        /* retreive ingress connection set information */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        connection.voq = c5_gport_ucast_queue_group0[idx];
        connection.remote_modid = c5_my_modid + mapping_info.core;
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(c5_gport_ucast_queue_group0[idx], 1, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c5_gport_ucast_queue_group0[idx]));
        rc = bcm_cosq_gport_connection_get(unit, &connection);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_get on Ingress, VoQ (0x%x) failed, error (%d)\n", c5_gport_ucast_queue_group0[idx], rc);
            return(rc);
        }
        if (connection.voq_connector != c5_gport_ucast_voq_connector_group0[idx]) {
            printf("gport_connection_get on Ingress, VoQ (0x%x) connector association mismatch, Expected (0x%x), retreived (0x%x)\n", c5_gport_ucast_queue_group0[idx], c5_gport_ucast_voq_connector_group0[idx], connection.voq_connector);
        }
    /*} end of for loop */

    printf("cint_e2e_composite_connectors.c (verify) completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}

/* Traffic passes after this step */
/*e2e_composite_connectors_setup(500000, 12);*/

/* Traffic stops after this step */
/* e2e_composite_connectors_teardown(12); */

/* verify setup */
/*e2e_composite_connectors_verify(500000, 12);*/

